import { instantiate } from "../wabt-wrapper.js";

(async function () {
    try {
    let instance = await instantiate(`
        (module
            (func (export "fence") (atomic.fence))
        )
        `, {}, { threads: true });
    for (var i = 0; i < wasmTestLoopCount; ++i)
        instance.exports.fence();
    } catch (e) {
        print(String(e));
    }
}());
