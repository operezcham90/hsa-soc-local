const exec = require('child_process').exec;
const fs = require('fs');

const result_file = '/root/result.csv';
var start_cat = 1;
var start_vid = 1;

var categories = [];
var videos = [];
var video_count = 0;
var first_frame_index = 0;
var last_frame_index = 0;
var top_l_x = 0;
var top_l_y = 0;
var bottom_r_x = 0;
var bottom_r_y = 0;
var ann = [];

var ntp;
var nfp;
var nfn;
var f;
var tau;
var tau_bar;
var frame_count;

var summary;
var u0;
var v0;
var n;
var m;
var full_time;

function mount_drive() {
    const command = 'mount /dev/sda1 /mnt';
    exec(command, (error, stdout, stderr) => {
        check_drive();
    });
}

function check_drive() {
    const command = 'ls /mnt/alov/ann';
    exec(command, (error, stdout, stderr) => {
        categories = stdout.split('\n');
        categories.pop();
        check_folder(0);
    });
}

function check_folder(category) {
    const folder = '/mnt/alov/ann/' + categories[category];
    const command = 'ls ' + folder;
    exec(command, (error, stdout, stderr) => {
        const folder_videos = stdout.split('\n');
        folder_videos.pop();
        videos.push(folder_videos);
        video_count += folder_videos.length;
        category++;
        if (category < categories.length) {
            check_folder(category);
        } else {
            check_res_file();
        }
    });
}

function check_res_file() {
    fs.exists(result_file, function (exists) {
        if (exists) {
            fs.readFile(result_file, 'utf8', (err, text) => {
                // read progress
                const res_lines = text.split('\n');
                // remove empty line
                res_lines.pop();
                // read last vid
                const last_line = res_lines.pop();
                start_cat = (+last_line.split(',')[0]) - 1;
                start_vid = (+last_line.split(',')[1]) - 1;
                // start next vid
                start_vid++;
                if (start_vid < videos[start_cat].length) {
                    read_ann(start_cat, start_vid);
                } else if (start_cat + 1 < categories.length) {
                    read_ann(start_cat + 1, 0);
                }
            });
        } else {
            save_res_file();
        }
    });
}

function save_res_file() {
    fs.writeFile(result_file, 'c,v,f,tau\n', (err) => {
        read_ann(0, 0);
    });
}

function read_ann(category, video) {
    const file = '/mnt/alov/ann/' + categories[category] + '/' + videos[category][video];
    fs.readFile(file, 'utf8', (err, data) => {
        const frames = data.split('\n');
        const first_frame_data = frames[0].split(/\s/g);
        const last_frame_data = frames[frames.length - 2].split(/\s/g);
        first_frame_index = +first_frame_data[0];
        last_frame_index = +last_frame_data[0];

        top_l_x = Math.min(+first_frame_data[1], +first_frame_data[3], +first_frame_data[5], +first_frame_data[7]);
        top_l_y = Math.min(+first_frame_data[2], +first_frame_data[4], +first_frame_data[6], +first_frame_data[8]);
        bottom_r_x = Math.max(+first_frame_data[1], +first_frame_data[3], +first_frame_data[5], +first_frame_data[7]);
        bottom_r_y = Math.max(+first_frame_data[2], +first_frame_data[4], +first_frame_data[6], +first_frame_data[8]);

        top_l_x = Math.floor(top_l_x);
        top_l_y = Math.floor(top_l_y);
        bottom_r_x = Math.ceil(bottom_r_x);
        bottom_r_y = Math.ceil(bottom_r_y);

        ann = [];
        global.gc();
        for (var i = 1; i <= frames.length - 2; i++) {
            const line = frames[i].split(/\s/g);
            const gt = {
                frame: +line[0],
                ax: +line[1],
                ay: +line[2],
                bx: +line[3],
                by: +line[4],
                cx: +line[5],
                cy: +line[6],
                dx: +line[7],
                dy: +line[8]
            };
            gt.u = Math.min(gt.ax, gt.bx, gt.cx, gt.dx);
            gt.v = Math.min(gt.ay, gt.by, gt.cy, gt.dy);
            gt.w = Math.max(gt.ax, gt.bx, gt.cx, gt.dx) - gt.u;
            gt.h = Math.max(gt.ay, gt.by, gt.cy, gt.dy) - gt.v;
            ann.push(gt);
        }

        ntp = 1;
        nfp = 0;
        nfn = 0;
        tau = 0;
        tau_bar = 0;
        frame_count = 0;

        set_frame_run(category, video, first_frame_index + 1);
    });
}

function get_frame_path(category, video, frame) {
    const c = categories[category];
    const v = ('00000' + (video + 1)).slice(-5);
    const f = ('00000000' + frame).slice(-8)
    return '/mnt/alov/frames/' + c + '/' + c + '_video' + v + '/' + f + '.jpg';
}

function set_frame_run(category, video, current_frame) {
    const t_file = get_frame_path(category, video, current_frame - 1);
    const i_file = get_frame_path(category, video, current_frame);
    const temp_t = '/root/hsa-soc-local/img/temp_t.txt';
    const temp_i = '/root/hsa-soc-local/img/temp_i.txt';
    const temp_tlx = '/root/hsa-soc-local/img/temp_tlx.txt';
    const temp_tly = '/root/hsa-soc-local/img/temp_tly.txt';
    const temp_brx = '/root/hsa-soc-local/img/temp_brx.txt';
    const temp_bry = '/root/hsa-soc-local/img/temp_bry.txt';
    fs.writeFile(temp_t, t_file, (err) => {
        fs.writeFile(temp_i, i_file, (err) => {
            fs.writeFile(temp_tlx, top_l_x.toString(), (err) => {
                fs.writeFile(temp_tly, top_l_y.toString(), (err) => {
                    fs.writeFile(temp_brx, bottom_r_x.toString(), (err) => {
                        fs.writeFile(temp_bry, bottom_r_y.toString(), (err) => {
                            do_frame_run(category, video, current_frame);
                        });
                    });
                });
            });
        });
    });
}

function do_frame_run(category, video, current_frame) {
    console.log(category + ' ' + video + ' ' + current_frame + ' / ' + last_frame_index);
    const command = '/root/hsa-soc-local/cpp/test_bee';
    exec(command, (error, stdout, stderr) => {
        try {
            summary = stdout.split('\n');
            u0 = +summary[9].split(':')[1];
            v0 = +summary[10].split(':')[1];
            n = +summary[11].split(':')[1];
            m = +summary[12].split(':')[1];
            full_time = +(summary[13].split(':')[1].replace(' us', ''));
            tau += full_time;
            frame_count++;
            top_l_x = u0;
            top_l_y = v0;
            bottom_r_x = top_l_x + n;
            bottom_r_y = top_l_y + m;

            // f-score
            for (var i = 0; i < ann.length; i++) {
                if (ann[i].frame === current_frame) {
                    const gt = ann[i];
                    const t = {
                        u: u0,
                        v: v0,
                        w: n,
                        h: m
                    };
                    const x_overlap = Math.max(0, Math.min(t.u + t.w - 1, gt.u + gt.w) - Math.max(t.u + 1, gt.u));
                    const y_overlap = Math.max(0, Math.min(t.v + t.h - 1, gt.v + gt.h) - Math.max(t.v + 1, gt.v));
                    const intersection = x_overlap * y_overlap;
                    const union = (t.w * t.h) + (gt.w * gt.h) - intersection;
                    const pascal = (intersection / union) >= 0.5;
                    if (pascal) {
                        ntp++;
                    } else {
                        nfp++;
                    }
                    const precision = ntp / (ntp + nfp);
                    const recall = ntp / (ntp + nfn);
                    f = (2 * precision * recall) / (precision + recall);
                    tau_bar = ((tau / frame_count) / 1000000);
                    break;
                }
            }

            current_frame++;
            if (current_frame <= last_frame_index) {
                set_frame_run(category, video, current_frame);
            } else {
                report_res(category, video);
            }
        } catch (error) {
            // reboot and retry
        }
    });
}

function report_res(category, video) {
    const new_line = (category + 1) + ',' + (video + 1) + ',' + f + ',' + tau_bar + '\n';
    fs.appendFile(result_file, new_line, function (err) {
        const html_file = '/root/hsa-soc-local/html/index.html';
        var html = 'last alive: ' + (new Date().toISOString()) + '<br>';
        html += 'last vid: ' + videos[category][video] + '<br>';
        fs.writeFile(html_file, html, (err) => {
            // reboot and next video
        });
    });
}

mount_drive();
