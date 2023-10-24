const p = require("./dist/mac-power-monitor.node");

(async () => {
    p.startListen();
    await new Promise((resolve) => setTimeout(resolve, 1000 * 60 * 60 * 24));

})().catch((error) => console.error(error));