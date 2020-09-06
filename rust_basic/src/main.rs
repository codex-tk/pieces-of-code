// https://rust-lang.github.io/async-book/04_pinning/01_chapter.html

use std::pin::Pin;
use std::marker::PhantomPinned;

#[derive(Debug)]
struct Test {
    a: String,
    b: *const String,
}

impl Test {
    fn new(txt: &str) -> Self {
        Test {
            a: String::from(txt),
            b: std::ptr::null(),
        }
    }

    fn init(&mut self) {
        self.b = &self.a
    }

    fn a(&self) -> &str {
        &self.a
    }

    fn b(&self) -> &String {
        unsafe { &*(self.b) }
    }
}


#[derive(Debug)]
struct StackPinTest{
    a: String,
    b: *const String,
    _marker: PhantomPinned,
}

impl StackPinTest {
    fn new(txt: &str) -> Self {
        StackPinTest {
            a: String::from(txt),
            b: std::ptr::null(),
            _marker: PhantomPinned,
        }
    }

    fn init(self: Pin<&mut Self>) {
        let self_ptr: *const String = &self.a;
        let this = unsafe{
            self.get_unchecked_mut()
        };
        this.b = self_ptr;
    }

    fn a(self: Pin<&Self>) -> &str {
        &self.get_ref().a
    }

    fn b(self: Pin<&Self>) -> &String {
        unsafe { &*(self.b) }
    }
}


#[derive(Debug)]
struct HeapPinTest{
    a: String,
    b: *const String,
    _marker: PhantomPinned,
}

impl HeapPinTest {
    fn new(txt: &str) -> Pin<Box<Self>> {
        let t = HeapPinTest {
            a: String::from(txt),
            b: std::ptr::null(),
            _marker: PhantomPinned,
        };
        let mut boxed = Box::pin(t);
        let self_ptr: *const String = &boxed.as_ref().a;
        unsafe {
            boxed.as_mut().get_unchecked_mut().b = self_ptr
        };
        boxed
    }

    fn a(self: Pin<&Self>) -> &str {
        &self.get_ref().a
    }

    fn b(self: Pin<&Self>) -> &String {
        unsafe { &*(self.b) }
    }
}



fn main() {
    let mut test1 = Test::new("test1");
    test1.init();
    let mut test2 = Test::new("test2");
    test2.init();
    println!("a: {}, b: {}", test1.a(), test1.b());
    std::mem::swap(&mut test1, &mut test2);
    println!("a: {}, b: {}", test2.a(), test2.b());

    let mut sptest1 = StackPinTest::new("sp1");
    let mut sptest1 = unsafe { Pin::new_unchecked(&mut sptest1)};
    //sptest1.init();
    StackPinTest::init(sptest1.as_mut());
    let mut sptest2 = StackPinTest::new("sp2");
    let mut sptest2 = unsafe { Pin::new_unchecked(&mut sptest2)};
    //sptest2.init();
    StackPinTest::init(sptest2.as_mut());
    println!("a: {}, b: {}", sptest1.as_ref().a(), sptest1.as_ref().b());
    //std::mem::swap(sptest1.get_mut(),sptest2.get_mut());
    println!("a: {}, b: {}", sptest2.as_ref().a(), sptest2.as_ref().b());

    let test1 = HeapPinTest::new("test1");
    let test2 = HeapPinTest::new("test2");

    println!("a: {}, b: {}",test1.as_ref().a(), test1.as_ref().b());
    println!("a: {}, b: {}",test2.as_ref().a(), test2.as_ref().b());
}
