(async () => {
    const monitor = await require("./lib/index").createMacPowerMonitor();
    await sleep(1000);

    console.log("start register callbacks");

    monitor.listenOnWillSleep(() => {
        console.log("say I will sleep");
    });
    monitor.listenOnWillWake(() => {
        console.log("say I will wake");
    });
    await sleep(1000 * 60 * 60 * 24);

})().catch((error) => console.error(error));

function sleep(timeDuration) {
    return new Promise((resolve) => setTimeout(resolve, timeDuration));
}