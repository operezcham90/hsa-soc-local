const { exec } = require('child_process');
var categories = [];
var videos = [];
var video_count = 0;
function mount_drive() {
    exec('mount /dev/sda1 /mnt', (error, stdout, stderr) => {
        check_drive();
    });
}
function check_drive() {
    exec('ls /mnt/alov/ann', (error, stdout, stderr) => {
        categories = stdout.split('\n');
        categories.pop();
        check_folder(0);
    });
}
function check_folder(category) {
    var folder = '/mnt/alov/ann/' + categories[category];
    exec('ls ' + folder, (error, stdout, stderr) => {
        var folder_videos = stdout.split('\n');
        folder_videos.pop();
        videos.push(folder_videos);
        video_count += folder_videos.length;
        console.log(video_count);
        category++;
        if (category < categories.length) {
            check_folder(category);
        }
    });
}
mount_drive();