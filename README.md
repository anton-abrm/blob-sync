# Blob Sync

A CLI application for Mac and Linux which simplifies storing blobs in Git

## Motivation

Many existing solutions like Git LFS suffer from the following drawbacks:

* Require custom software to be deployed on the remote side
* Don't use SSH as a transport protocol which makes it impossible to use SSH agent for authentication
* Difficult or confusing to configure

## Solution

The main idea is to store only the file structure in the git repository and synchronize all the changes with the corresponding content directory where the actual files are located.
All the synchronization between the working directory and the content directory are handled by this tool.
The synchronization with the remote storage are done by using `rsync` under the hood which allows to store the blobs in different types of remote locations.

## Configuration

The following changes should be made in `.git/config`

```
[blob-sync]
    content-dir = /home/user/music # Local directory with the actual files
    storage-url = user@sever.dev:music.bls # Remote directory where all the blobs are located
```

## Dependencies

* [libgit2](https://libgit2.org/)
* [OpenSSL](https://www.openssl.org/)
* [rsync](https://github.com/RsyncProject/rsync/)

