const exec = require('child_process').exec;
const fs = require('fs');

const command_run = 'node --expose-gc run.js';
const command_rest = 'reboot';
const times_file = '/root/times';
const max_times = 3;

var times = 0;

function check_times() {
    fs.exists(times_file, function (exists) {
        if (exists) {
            fs.readFile(times_file, 'utf8', (err, text) => {
                times = +text;
                times++;
                save_times();
            });
        } else {
            save_times();
        }
    });
}

function save_times() {
    if (times < max_times) {
        fs.writeFile(times_file, times, err => {
            run();
        });
    }
}

function run() {
    exec(command_run, (error, stdout, stderr) => {
        fs.writeFile('/root/dump', stdout + stderr, err => {
            rest();
        });
    });
}

function rest() {
    exec(command_rest, (error, stdout, stderr) => {
    });
}

check_times();