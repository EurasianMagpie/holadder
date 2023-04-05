package main

import (
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
	cmd := fmt.Sprintf("./holadder %s %s", param.GetGlobalParams().ServiceName, param.GetGlobalParams().BatchCount)
	fmt.Println("commandline:", cmd)
	exec.Command("./holadder", param.GetGlobalParams().ServiceName, param.GetGlobalParams().BatchCount)
	handleQueryCurrentPort(c)
}

func startHoladderServer() {
	eng := gin.Default()

	apisubdomain := eng.Group("/holadder")
	apisubdomain.GET("/query", handleQueryCurrentPort)
	apisubdomain.GET("/move", handleMovePort)

	eng.Run("127.0.0.1:8080")
}

func main() {
	startHoladderServer()
}
