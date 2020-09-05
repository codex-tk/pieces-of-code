// from https://www.viget.com/articles/understanding-futures-in-rust-part-1/
use std::cell::RefCell;

thread_local! {static NOTIFY: RefCell<bool> = RefCell::new(true)}

mod custom_task {
    use crate::NOTIFY;

    pub struct Context<'a> {
        waker: &'a Waker,
    }

    impl<'a> Context<'a> {
        pub fn from_waker(waker: &'a Waker) -> Self {
            Context { waker }
        }

        pub fn waker(&self) -> &'a Waker {
            &self.waker
        }
    }

    pub struct Waker;

    impl Waker {
        pub fn wake(&self) {
            NOTIFY.with(
                |f| *f.borrow_mut() = true
            )
        }
    }
}

use crate::custom_task::*;

mod custom_future {
    use crate::custom_task::*;

    pub enum Poll<T> {
        Ready(T),
        Pending,
    }

    pub trait CustomFuture {
        type Output;
        fn poll(&mut self, ctx: &Context) -> Poll<Self::Output>;

        fn map<F,T>(self, f: F) -> Map<Self,F>
        where
            F: FnOnce(Self::Output)->T,
            Self: Sized,
        {
            Map{
                future: self,
                func: Some(f),
            }
        }

        fn then<F,NextFut>(self, f: F) -> Then<Self,F>
        where
            F: FnOnce(Self::Output)->NextFut,
            NextFut: CustomFuture,
            Self: Sized,
        {
            Then{
                future: self,
                func: Some(f),
            }
        }
    }

    pub trait TryFuture{
        type Ok;
        type Error;

        fn try_poll(&mut self, ctx: &Context)->Poll<Result<Self::Ok, Self::Error>>;

        fn and_then<Fut, F>(self, f: F) -> AndThen<Self,F>
        where
            F: FnOnce(Self::Ok) -> Fut,
            Fut: CustomFuture,
            Self: Sized
        {
            AndThen{
                future: self,
                func: Some(f)
            }
        }
    }

    impl<F,T,E> TryFuture for F
    where
        F:CustomFuture<Output=Result<T,E>>
    {
        type Ok = T;
        type Error = E;

        fn try_poll(&mut self, ctx: &Context) -> Poll<Result<Self::Ok, Self::Error>> {
            self.poll(ctx)
        }
    }

    pub struct Ready<T>(Option<T>);

    impl<T> CustomFuture for Ready<T> {
        type Output = T;

        fn poll(&mut self, _: &Context) -> Poll<Self::Output> {
            Poll::Ready(self.0.take().unwrap())
        }
    }

    pub fn ready<T>(val: T) -> Ready<T> {
        Ready(Some(val))
    }

    pub struct Map<Fut,F> {
        future: Fut,
        func: Option<F>,
    }

    impl<Fut,F,T> CustomFuture for Map<Fut,F>
    where
        Fut: CustomFuture,
        F: FnOnce(Fut::Output)->T,
    {
        type Output = T;

        fn poll(&mut self, ctx: &Context) -> Poll<Self::Output> {
            match self.future.poll(ctx){
                Poll::Ready(val)=>{
                    let func = self.func.take().unwrap();
                    Poll::Ready(func(val))
                },
                Poll::Pending => Poll::Pending,
            }
        }
    }

    pub struct Then<Fut,F> {
        future: Fut,
        func: Option<F>,
    }

    impl<Fut,F,NextFut> CustomFuture for Then<Fut,F>
    where
        Fut: CustomFuture,
        F: FnOnce(Fut::Output)->NextFut,
        NextFut: CustomFuture,
    {
        type Output = NextFut::Output;

        fn poll(&mut self, ctx: &Context) -> Poll<Self::Output> {
            match self.future.poll(ctx){
                Poll::Ready(val)=>{
                    let func = self.func.take().unwrap();
                    func(val).poll(ctx)
                },
                Poll::Pending => Poll::Pending,
            }
        }
    }

    pub struct AndThen<Fut,F>{
        future: Fut,
        func: Option<F>,
    }

    impl<Fut,F,NextFut> CustomFuture for AndThen<Fut,F>
    where
        Fut: TryFuture,
        F: FnOnce(Fut::Ok)->NextFut,
        NextFut: TryFuture<Error=Fut::Error>
    {
        type Output = Result<NextFut::Ok,NextFut::Error>;

        fn poll(&mut self, ctx: &Context) -> Poll<Self::Output> {
            match self.future.try_poll(ctx){
                Poll::Ready(Ok(val)) =>{
                    let f = self.func.take().unwrap();
                    f(val).try_poll(ctx)
                },
                Poll::Ready(Err(e)) => Poll::Ready(Err(e)),
                Poll::Pending => Poll::Pending
            }
        }
    }


}

use crate::custom_future::*;

#[derive(Default)]
struct MyFuture { count: u32 }

impl CustomFuture for MyFuture {
    type Output = i32;

    fn poll(&mut self, ctx: &Context) -> Poll<Self::Output> {
        match self.count {
            3 => Poll::Ready(3),
            _ => {
                self.count += 1;
                ctx.waker().wake();
                Poll::Pending
            }
        }
    }
}

struct AddOneFuture<T>(T);

impl<T> CustomFuture for AddOneFuture<T>
    where
        T: CustomFuture,
        T::Output: std::ops::Add<i32, Output=i32>
{
    type Output = i32;

    fn poll(&mut self, ctx: &Context) -> Poll<Self::Output> {
        match self.0.poll(ctx) {
            Poll::Ready(val) => Poll::Ready(val + 1),
            Poll::Pending => Poll::Pending
        }
    }
}

fn run<F>(mut f: F) -> F::Output
    where
        F: CustomFuture
{
    NOTIFY.with(|n| {
        { *n.borrow_mut() = true; }
        loop {
            if *n.borrow() {
                *n.borrow_mut() = false;
                let ctx = Context::from_waker(&Waker);

                if let Poll::Ready(val) = f.poll(&ctx) {
                    return val;
                }
            }
        }
    })
}


fn main() {
    let my_future: MyFuture = Default::default();
    println!("Output 0: {}", run(my_future));
    let my_future: MyFuture = Default::default();
    println!("Output 1: {}", run(AddOneFuture(my_future)));
    let ready_future = custom_future::ready(10);
    println!("Output 2: {}", run(ready_future));
    let map_future = custom_future::ready(10).map(
        |val| val * 2
    );
    println!("Output 3: {}", run(map_future));
    let then_future = custom_future::ready(1)
        .map(|v|v*10)
        .then(|v| custom_future::ready(v+2));
    println!("Output 4: {}", run(then_future));

    let and_then_future = custom_future::ready(1)
        .map(|val| val + 1)
        .then(|val| custom_future::ready(val + 1))
        .map(Ok::<i32,()>)
        .and_then(|val| custom_future::ready(Ok(val + 1)));
    println!("Output 5: {:?}", run(and_then_future));
}
