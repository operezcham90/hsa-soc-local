const command_work = 'node --expose-gc run.js';
const command_rest = 'reboot';

const exec = require('child_process').exec;
const fs = require('fs');

function work() {
    exec(command_work, (error, stdout, stderr) => {
        rest();
    });
}

function rest() {
    exec(command_rest, (error, stdout, stderr) => {
    });
}