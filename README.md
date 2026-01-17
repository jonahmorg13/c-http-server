# C Http Server
> An HTTP server written in C

## Data Structures Used
* _Radix Tree_ - Used for mapping endpoints with function handlers. Never used this DS before. Pretty interesting to see a use case for it!
* _Vectors_ - Used for handling middleware dispatch calling

I wanted to learn how to create a http server from hand. I learned a LOT about memory management...

_Figure 1: an example of exposing static files over a socket_
<img width="2378" height="1360" alt="image" src="https://github.com/user-attachments/assets/1cf45fea-f5ac-4628-8180-01d2101ba757" />

## Build & Run
```sh
./compile.sh # build
bin/server   # run
```

Note: Yes, the todo app static file was vibe coded. I just wanted to see if it worked with static files!
