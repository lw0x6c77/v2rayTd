# !/bin/bash

set -o nounset

do_enable_proxy()
{
    local dev="$1"
    local http_proxy="$2"
    local socks_proxy="$3"

    networksetup -setwebproxy wi-fi $http_proxy > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setwebproxy info error"
        exit 1
    fi

    networksetup -setwebproxystate wi-fi on > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setwebproxystate on error"
        exit 1
    fi

    networksetup -setsecurewebproxy wi-fi $http_proxy > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setsecurewebproxy info error"
        exit 1
    fi

    networksetup -setsecurewebproxystate wi-fi on > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setsecurewebproxystate on error"
        exit 1
    fi

    networksetup -setsocksfirewallproxy wi-fi $socks_proxy > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setsocksfirewallproxy info error"
        exit 1
    fi

    networksetup -setsocksfirewallproxystate wi-fi on > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setsocksfirewallproxystate on error"
        exit 1
    fi

    echo "enable system proxy [$1] successed"
}

enable_proxy()
{
    if [[ $# -ne 2 ]]; then
        echo "command 'on' need 'http_proxy' 'socks_proxy'"
        echo "usage tool.sh on 127.0.0.1:10808 127.0.0.1:10809"
        exit 1
    fi

    local http_proxy=$(echo $1 | awk -F ':' '{printf("%s %s", $1, $2)}')
    local socks_proxy=$(echo $2 | awk -F ':' '{printf("%s %s", $1, $2)}')
    echo "[http] $http_proxy"
    echo "[socks] $socks_proxy"

    local hardwares=$(networksetup -listallhardwareports)
    echo "$hardwares" | grep "Hardware Port: Wi-Fi" > /dev/null 2>&1
    if [[ $? -eq 0 ]]; then
        do_enable_proxy "Wi-Fi" "$http_proxy" "$socks_proxy"
    fi

    echo "$hardwares" | grep "Hardware Port: Ethernet" > /dev/null 2>&1
    if [[ $? -eq 0 ]]; then
        do_enable_proxy "Ethernet" "$http_proxy" "$socks_proxy"
    fi

    echo "$hardwares" | grep "Hardware Port: AirPort" > /dev/null 2>&1
    if [[ $? -eq 0 ]]; then
        do_enable_proxy "AirPort" "$http_proxy" "$socks_proxy"
    fi
}

disable_proxy()
{
    networksetup -setwebproxystate wi-fi off > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setwebproxystate off error"
        exit 1
    fi

    networksetup -setsecurewebproxystate wi-fi off > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setsecurewebproxystate off error"
        exit 1
    fi

    networksetup -setsocksfirewallproxystate wi-fi off > /dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "setsocksfirewallproxysate state error"
        exit 1
    fi

    echo "disable system proxy successed"
    exit 0
}

usage()
{
    echo "tool.sh on | off [args]"
    exit 1
}

enable_terminal_proxy()
{
    export http_proxy="$1"
    export https_proxy=$http_proxy
    export socks_proxy="$2"
}

disable_terminal_proxy()
{
    unset http_proxy
    unset https_proxy
    unset socks_proxy
}

update_geosite()
{
    local msg
    local src="site.dat"
    local dest="geosite.dat"

    if [[ $# -ne 3 ]]; then
        echo "command 'update_geosite' need 'http_proxy' 'socks_proxy' 'url'"
        echo "usage tool.sh update_geosite 127.0.0.1:10808 127.0.0.1:10809 'download url'"
        exit 1
    fi

    enable_terminal_proxy "$1" "$2"
    rm -f $src
    msg=$(curl -s -L -o $src --connect-timeout 5 -w "%{http_code}" "$3" 2>&1)
    if [[ $? -ne 0 || "$msg" != "200" ]]; then
        echo "download $3 failed:\n$msg"
        exit 1
    fi
    disable_terminal_proxy
    if [[ -r "$src" ]]; then
        mv $src $dest
        rm -f $src
    else
        echo "$src: no such file or directory"
        exit 1
    fi
    echo "update geosite successed"
}

update_geoip()
{
    local msg
    local src="ip.dat"
    local dest="geoip.dat"

    if [[ $# -ne 3 ]]; then
        echo "command 'update_geoip' need 'http_proxy' 'socks_proxy' 'url'"
        echo "usage tool.sh update_geoip 127.0.0.1:10808 127.0.0.1:10809 'download url'"
        exit 1
    fi

    enable_terminal_proxy "$1" "$2"
    rm -f $src
    msg=$(curl -s -L -o $src --connect-timeout 5 -w "%{http_code}" "$3" 2>&1)
    if [[ $? -ne 0 || "$msg" != "200" ]]; then
        echo "download $3 failed:\n$msg"
        exit 1
    fi
    disable_terminal_proxy
    if [[ -r "$src" ]]; then
        mv $src $dest
        rm -f $src
    else
        echo "$src: no such file or directory"
        exit 1
    fi
    echo "update geoip successed"
}

case "$1" in
    "on")
        enable_proxy "$2" "$3"
        exit 0
        ;;
    "off")
        disable_proxy
        exit 0
        ;;
    "update_geosite")
        update_geosite "$2" "$3" "$4"
        exit 0
        ;;
    "update_geoip")
        update_geoip "$2" "$3" "$4"
        exit 0
        ;;
    *)
        usage
        ;;
esac
