const { exec } = require('child_process');
const fs = require('fs');
var categories = [];
var videos = [];
var video_count = 0;
var code_temp = '';
function mount_drive() {
    exec('mount /dev/sda1 /mnt', (error, stdout, stderr) => {
        fs.readFile('~/hsa-soc-local/cpp/test.cpp', 'utf8', (err, code) => {
            code_temp = code;
            check_drive();
        });
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
        console.log(file);
        var frames = data.split('\n');
        var first_frame_data = frames[0].split(/\s/g);
        var frame_index = +first_frame_data[0];
        var t_file = '/mnt/alov/frames/' + categories[category] + '/' + categories[category] + '_video';
        t_file += ('00000' + (video + 1)).slice(-5) + '/' + ('00000000' + frame_index).slice(-8) + '.jpg';
        var i_file = '/mnt/alov/frames/' + categories[category] + '/' + categories[category] + '_video';
        i_file += ('00000' + (video + 1)).slice(-5) + '/' + ('00000000' + frame_index + 1).slice(-8) + '.jpg';
        var top_l_x = +first_frame_data[1];
        if (+first_frame_data[3] < top_l_x) {
            top_l_x = +first_frame_data[3];
        }
        if (+first_frame_data[5] < top_l_x) {
            top_l_x = +first_frame_data[5];
        }
        if (+first_frame_data[7] < top_l_x) {
            top_l_x = +first_frame_data[7];
        }
        var top_l_y = +first_frame_data[2];
        if (+first_frame_data[4] < top_l_y) {
            top_l_y = +first_frame_data[4];
        }
        if (+first_frame_data[6] < top_l_y) {
            top_l_y = +first_frame_data[6];
        }
        if (+first_frame_data[8] < top_l_y) {
            top_l_y = +first_frame_data[8];
        }
        var bottom_r_x = +first_frame_data[1];
        if (+first_frame_data[3] > bottom_r_x) {
            bottom_r_x = +first_frame_data[3];
        }
        if (+first_frame_data[5] > bottom_r_x) {
            bottom_r_x = +first_frame_data[5];
        }
        if (+first_frame_data[7] > bottom_r_x) {
            bottom_r_x = +first_frame_data[7];
        }
        var bottom_r_y = +first_frame_data[2];
        if (+first_frame_data[4] > bottom_r_y) {
            bottom_r_y = +first_frame_data[4];
        }
        if (+first_frame_data[6] > bottom_r_y) {
            bottom_r_y = +first_frame_data[6];
        }
        if (+first_frame_data[8] > bottom_r_y) {
            bottom_r_y = +first_frame_data[8];
        }
        var const_code = '#define I_FILE "' + i_file + '"\n';
        const_code += '#define T_FILE "' + t_file + '"\n';
        const_code += '#define TOP_L_X ' + top_l_x + '\n';
        const_code += '#define TOP_L_Y ' + top_l_y + '\n';
        const_code += '#define BOTTOM_R_X ' + bottom_r_x + '\n';
        const_code += '#define BOTTOM_R_Y ' + bottom_r_y + '\n';
        fs.writeFile('~/hsa-soc-local/cpp/frame.cpp', const_code + code_temp, () => {
            console.log('written')
        });
    });
}
mount_drive();