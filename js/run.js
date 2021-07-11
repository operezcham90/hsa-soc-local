const { spawn } = require("child_process");

const mount = spawn("mount", ["/dev/sda1", "/mnt"]);

mount.stdout.on("data", data => {
    console.log(`stdout: ${data}`);
});

mount.stderr.on("data", data => {
    console.log(`stderr: ${data}`);
});

mount.on('error', (error) => {
    console.log(`error: ${error.message}`);
});

mount.on("close", code => {
    console.log(`child process exited with code ${code}`);
});