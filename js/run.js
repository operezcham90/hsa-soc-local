const { spawn } = require("child_process");

const umount = await spawn("umount /dev/sda1");
const mount = await spawn("mount /dev/sda1 /mnt");

mount.stdout.on("data", data => {
    console.log(`${data}`);
});