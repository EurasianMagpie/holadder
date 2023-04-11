package main

import (
	"flag"
	"fmt"
	"holadderServer/conf"
	"holadderServer/param"
	"holadderServer/resp"
	"os/exec"

	"github.com/gin-gonic/gin"
)

// http://localhost:8080/holadder/query
func handleQueryCurrentPort(c *gin.Context) {
	port, err := conf.GetCurrentPortFromConf(param.GetGlobalParams().ConfFile)
	if err == nil {
		d := gin.H{
			"port": port,
		}
		c.JSON(200, resp.FormResult(0, "", d))
	} else {
		c.JSON(200, resp.FormResult(700, string("there was something wrong."), gin.H{}))
	}
}

// http://localhost:8080/holadder/move
func handleMovePort(c *gin.Context) {
	cmdline := fmt.Sprintf("./holadder %s %s", param.GetGlobalParams().ServiceName, param.GetGlobalParams().BatchCount)
	fmt.Println("commandline:", cmdline)
	cmd := exec.Command("./holadder", param.GetGlobalParams().ServiceName, param.GetGlobalParams().BatchCount)
	err := cmd.Run()
	if err != nil {
		c.JSON(200, resp.FormResult(701, string("execute command failed."), gin.H{}))
		return
	}

	handleQueryCurrentPort(c)
}

func startHoladderServer(port string) {
	eng := gin.Default()

	apisubdomain := eng.Group("/holadder")
	apisubdomain.GET("/query", handleQueryCurrentPort)
	apisubdomain.GET("/move", handleMovePort)

	addr := "127.0.0.1:" + port
	eng.Run(addr)
}

func main() {
	port := flag.String("port", "8080", "specify listen port")
	flag.Parse()
	startHoladderServer(*port)
}
