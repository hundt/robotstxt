# Forked version of Google Robots.txt Parser and Matcher Library

The repository contains Google's robots.txt parser and matcher as a C++ library
(compliant to C++11) and a web server serving an API for dealing with robots.txt files.

## Building

docker build -t robots:dev .

## Running

docker run -p 8080:80 robots:dev

## Usage

### /check
This endpoint rewrites a robots.txt file, commenting out invalid or ignored lines and canonicalizing remaining lines.

`curl -F robots=@robots.txt http://localhost:8080/check`

### /urls
This endpoint takes a robots.txt file, a user-agent, and a file with one URL on each line, and echoes back the list of URLs with "1" or "0" prepended to each line, indicating whether that user-agent would be allowed to crawl that URL.

`curl -F userAgent='NinjaBot' -F robots=@robots.txt -F urls=@urls.txt http://localhost:8080/urls`
