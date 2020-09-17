// https://rust-lang.github.io/async-book/04_pinning/01_chapter.html

mod mybox;
mod pin_test;

use mybox::*;
use pin_test::*;

use std::pin::Pin;

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

    let mybox = MyBox::new(3);
    println!("{:?} {}", mybox, (*mybox));

    let x = 5;
    let y = MyBox::new(x);

    assert_eq!(5, x);
    assert_eq!(5, *y);
}
