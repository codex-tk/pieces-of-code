// from https://www.viget.com/articles/understanding-futures-in-rust-part-1/
use std::cell::RefCell;

thread_local! {static NOTIFY: RefCell<bool> = RefCell::new(true)}

struct Context<'a> {
    waker: &'a Waker,
}

impl <'a> Context<'a>{
    fn from_waker(waker: &'a Waker) -> Self{
        Context{ waker }
    }

    fn waker(&self) -> &'a Waker{
        &self.waker
    }
}

struct Waker;

impl Waker {
    fn wake(&self){
        NOTIFY.with(
            |f| *f.borrow_mut() = true
        )
    }
}

fn run<F>( mut f: F ) -> F::Output
where
    F: CustomFuture
{
    NOTIFY.with(|n| loop{
        if *n.borrow() {
            *n.borrow_mut() = false;
            let ctx = Context::from_waker(&Waker);
            if let Poll::Ready(val) = f.poll(&ctx){
                return val
            }
        }
    })
}

enum Poll<T> {
    Ready(T),
    Pending
}

trait CustomFuture{
    type Output;
    fn poll(&mut self, ctx: &Context) -> Poll<Self::Output>;
}

#[derive(Default)]
struct MyFuture{ count: u32, }

impl CustomFuture for MyFuture{
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


fn main() {
    let my_future : MyFuture = Default::default();
    println!("Output: {}", run(my_future));
}
