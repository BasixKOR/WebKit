function foo(a0) {
    for (let i = 0; i < 100; i++) {
        if (!a0) {
            foo({});
        }
        for (let j = 0; j < testLoopCount; j++) {}
    }
    a0.a;
}
foo(0);
