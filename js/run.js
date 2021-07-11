const { spawn } = require("child_process");

const mount = spawn("mount", ["/dev/sda1", "/mnt"]);
mount.stdout.on("data", data => {
    const ls = spawn("ls", ["/mnt/alov"]);
    ls.stdout.on("data", data => {
        console.log(data);
    });
});