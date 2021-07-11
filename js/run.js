const { exec } = require('child_process');
var categories = [];
function mount_drive() {
    exec('mount /dev/sda1 /mnt', (error, stdout, stderr) => {
        check_drive();
    });
}
function check_drive() {
    exec('ls /mnt/alov/ann', (error, stdout, stderr) => {
        categories = stdout.split('\n');
        console.log(JSON.stringify(categories));
    });
}
mount_drive();