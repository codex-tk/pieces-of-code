
use std::pin::Pin;
use std::marker::PhantomPinned;

#[derive(Debug)]
pub(crate) struct Test {
    a: String,
    b: *const String,
}

impl Test {
    pub fn new(txt: &str) -> Self {
        Test {
            a: String::from(txt),
            b: std::ptr::null(),
        }
    }

    pub fn init(&mut self) {
        self.b = &self.a
    }

    pub fn a(&self) -> &str {
        &self.a
    }

    pub fn b(&self) -> &String {
        unsafe { &*(self.b) }
    }
}


#[derive(Debug)]
pub struct StackPinTest{
    a: String,
    b: *const String,
    _marker: PhantomPinned,
}

impl StackPinTest {
    pub fn new(txt: &str) -> Self {
        StackPinTest {
            a: String::from(txt),
            b: std::ptr::null(),
            _marker: PhantomPinned,
        }
    }

    pub fn init(self: Pin<&mut Self>) {
        let self_ptr: *const String = &self.a;
        let this = unsafe{
            self.get_unchecked_mut()
        };
        this.b = self_ptr;
    }

    pub fn a(self: Pin<&Self>) -> &str {
        &self.get_ref().a
    }

    pub fn b(self: Pin<&Self>) -> &String {
        unsafe { &*(self.b) }
    }
}


#[derive(Debug)]
pub struct HeapPinTest{
    a: String,
    b: *const String,
    _marker: PhantomPinned,
}

impl HeapPinTest {
    pub fn new(txt: &str) -> Pin<Box<Self>> {
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

    pub fn a(self: Pin<&Self>) -> &str {
        &self.get_ref().a
    }

    pub fn b(self: Pin<&Self>) -> &String {
        unsafe { &*(self.b) }
    }
}
