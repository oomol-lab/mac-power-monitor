const p = require("./dist/mac-power-monitor.node");

(async () => {
    p.start();
    await sleep(1000);

    console.log("start register callbacks");

    p.registerOnWillSleep(() => {
        console.log("say I will sleep");
    });

    p.registerOnWillWake(() => {
        console.log("say I will wake");
        p.unregister();
    });

    await sleep(1000 * 60 * 60 * 24);

})().catch((error) => console.error(error));

function sleep(timeDuration) {
    return new Promise((resolve) => setTimeout(resolve, timeDuration));
}