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
    use std::process::Output;

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
    }

    pub struct Ready<T>(Option<T>);

    impl<T> CustomFuture for Ready<T> {
        type Output = T;

        fn poll(&mut self, ctx: &Context) -> Poll<Self::Output> {
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

}
