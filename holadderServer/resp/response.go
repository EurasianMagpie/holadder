package resp

import (
	"github.com/gin-gonic/gin"
)

func FormResult(errno int, errmsg string, data gin.H) gin.H {
	return gin.H{
		"errno":  errno,
		"errmsg": errmsg,
		"data":   data,
	}
}
