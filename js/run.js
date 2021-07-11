const { exec } = require('child_process');
function mount_drive() {
    exec('mount /dev/sda1 /mnt', (error, stdout, stderr) => {
        check_drive();
    });
}
function check_drive() {
    exec('ls /mnt/alov/ann', (error, stdout, stderr) => {
        console.log('folders:\n' + stdout);
    });
}
mount_drive();