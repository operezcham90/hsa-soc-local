/*const { exec } = require('child_process');
const http = require('http');
const fs = require('fs');

function compile(callback) {
    const { exec } = require('child_process');
    const http = require('http');
    const fs = require('fs');

    function compile(callback) {
        console.log('compilation...');
        exec('g++ /root/cpp/driver.cpp -o /root/cpp/driver `pkg-config --cflags --l$
        if (error || stderr) {
            console.log('compilation fail');
        }
        console.log(stdout);
        callback();
    });
}

function run() {
    exec('/root/cpp/driver', (error, stdout, stderr) => {
        if (error || stderr) {
            console.log('run fail'); console.log('compilation...');
            exec('g++ /root/cpp/driver.cpp -o /root/cpp/driver `pkg-config --cflags --libs opencv`', (error, stdout, stderr) => {
                if (error || stderr) {
                    console.log('compilation fail');
                }
                console.log(stdout);
                callback();
            });
        }

        function run() {
            exec('/root/cpp/driver', (error, stdout, stderr) => {
                if (error || stderr) {
                    console.log('run fail');
                }
                console.log(stdout);
            });
        }
        compile(run);

        http.createServer(function (request, response) {
            var stat = fs.statSync('/root/img/result.jpg');
            response.writeHead(200, {
                'Content-Type': 'image/jpg',
                'Content-Length': stat.size
            });
            var rs = fs.createReadStream('/root/img/result.jpg');
            rs.pipe(response);
        }).listen(80);*/