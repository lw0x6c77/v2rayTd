### v2rayTd

> mac os command line tool
1. v2rayTd: this is a daemon that manages the proxy engine.
2. v2rayTctl: this is a tool that for connecting to v2rayTd.
3. The proxy engine(v2ray|xray) process reads the configuration file directly and is flexible enough that you can configure whatever you need.
4. Before using this tool, you need to download the required files(v2ray|xray) to the current directory in advance.

---
. view mac os syslog
```
log stream --info --debug --predicate 'sender == "v2rayTd"' --style syslog
```
. https://stackoverflow.com/questions/22004505/where-do-debug-and-info-messages-go-when-using-pythons-sysloghandler-on-mac-osx

. https://stackoverflow.com/questions/49805750/macos-high-sierra-syslog-does-not-work
