const { spawn } = require("child_process");

const umount = await spawn("umount /dev/sda1");
umount.stdout.on("data", data => {
    const mount = await spawn("mount /dev/sda1 /mnt");
    mount.stdout.on("data", data => {
        console.log(data);
    });
});