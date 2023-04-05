package conf

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func GetCurrentPortFromConf(filepath string) (int, error) {
	port := 0
	file, err := os.Open(filepath)
	if err != nil {
		fmt.Println("conf can not open conf file:", filepath)
		return 0, err
	}
	defer file.Close()

	scaner := bufio.NewScanner(file)
	for scaner.Scan() {
		line := scaner.Text()
		if len(line) == 0 {
			continue
		} else if line[0] == '#' || line[0] == ';' {
			continue
		} else if strings.HasPrefix(line, "port") {
			v, err := strconv.Atoi(line[5:])
			if err == nil {
				port = v
				break
			}
		}
	}

	if err := scaner.Err(); err != nil {
		return 0, err
	}

	return port, nil
}
