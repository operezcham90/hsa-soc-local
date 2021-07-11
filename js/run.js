const { spawn } = require("child_process");

const umount = spawn("umount /dev/sda1");
umount.stdout.on("data", data => {
    const mount = spawn("mount /dev/sda1 /mnt");
    mount.stdout.on("data", data => {
        console.log(data);
    });
});