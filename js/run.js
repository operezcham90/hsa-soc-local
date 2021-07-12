const { exec } = require('child_process');
const fs = require('fs');

var categories = [];
var videos = [];
var video_count = 0;
var first_frame_index = 0;
var last_frame_index = 0;
var top_l_x = 0;
var top_l_y = 0;
var bottom_r_x = 0;
var bottom_r_y = 0;

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
        category++;
        if (category < categories.length) {
            check_folder(category);
        } else {
            read_ann(0, 0);
        }
    });
}

function read_ann(category, video) {
    var file = '/mnt/alov/ann/' + categories[category] + '/' + videos[category][video];
    fs.readFile(file, 'utf8', (err, data) => {
        var frames = data.split('\n');
        var first_frame_data = frames[0].split(/\s/g);
        var last_frame_data = frames[frames.length - 2].split(/\s/g);
        first_frame_index = +first_frame_data[0];
        last_frame_index = +last_frame_data[0];
        console.log(categories[category] + ' ' + video + ' ' + first_frame_index + '-' + last_frame_index);

        top_l_x = +first_frame_data[1];
        if (+first_frame_data[3] < top_l_x) {
            top_l_x = +first_frame_data[3];
        }
        if (+first_frame_data[5] < top_l_x) {
            top_l_x = +first_frame_data[5];
        }
        if (+first_frame_data[7] < top_l_x) {
            top_l_x = +first_frame_data[7];
        }

        top_l_y = +first_frame_data[2];
        if (+first_frame_data[4] < top_l_y) {
            top_l_y = +first_frame_data[4];
        }
        if (+first_frame_data[6] < top_l_y) {
            top_l_y = +first_frame_data[6];
        }
        if (+first_frame_data[8] < top_l_y) {
            top_l_y = +first_frame_data[8];
        }

        bottom_r_x = +first_frame_data[1];
        if (+first_frame_data[3] > bottom_r_x) {
            bottom_r_x = +first_frame_data[3];
        }
        if (+first_frame_data[5] > bottom_r_x) {
            bottom_r_x = +first_frame_data[5];
        }
        if (+first_frame_data[7] > bottom_r_x) {
            bottom_r_x = +first_frame_data[7];
        }

        bottom_r_y = +first_frame_data[2];
        if (+first_frame_data[4] > bottom_r_y) {
            bottom_r_y = +first_frame_data[4];
        }
        if (+first_frame_data[6] > bottom_r_y) {
            bottom_r_y = +first_frame_data[6];
        }
        if (+first_frame_data[8] > bottom_r_y) {
            bottom_r_y = +first_frame_data[8];
        }

        set_frame_run(category, video, first_frame_index + 1);
    });
}

function set_frame_run(category, video, current_frame) {
    var t_file = '/mnt/alov/frames/' + categories[category] + '/' + categories[category] + '_video';
    t_file += ('00000' + (video + 1)).slice(-5) + '/' + ('00000000' + (current_frame - 1)).slice(-8) + '.jpg';
    var i_file = '/mnt/alov/frames/' + categories[category] + '/' + categories[category] + '_video';
    i_file += ('00000' + (video + 1)).slice(-5) + '/' + ('00000000' + current_frame).slice(-8) + '.jpg';
    fs.copyFile(t_file, '/root/hsa-soc-local/img/temp_t.jpg', (err) => {
        fs.copyFile(i_file, '/root/hsa-soc-local/img/temp_i.jpg', (err) => {
            do_frame_run(category, video, current_frame);
        });
    });
}

function do_frame_run(category, video, current_frame) {
    fs.writeFile('/root/hsa-soc-local/img/temp_tlx.txt', top_l_x.toString(), (err) => {
        fs.writeFile('/root/hsa-soc-local/img/temp_tly.txt', top_l_y.toString(), (err) => {
            fs.writeFile('/root/hsa-soc-local/img/temp_brx.txt', bottom_r_x.toString(), (err) => {
                fs.writeFile('/root/hsa-soc-local/img/temp_bry.txt', bottom_r_y.toString(), (err) => {
                    console.log(category + ' ' + video + ' ' + current_frame);
                });
            });
        });
    });
}

mount_drive();