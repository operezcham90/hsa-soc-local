const { spawn } = require("child_process");

const mount = spawn("mount", ["/dev/sda1", "/mnt"]);
const ls = spawn("ls", ["/mnt/alov"]);
ls.stdout.on("data", data => {
    console.log(data);
});