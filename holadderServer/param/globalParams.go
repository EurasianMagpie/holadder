package param

import (
	"encoding/json"
	"fmt"
	"os"
)

type GlobalParams struct {
	ConfFile    string `json:"conffile"`
	ServiceName string `json:"servicename"`
	BatchCount  string `json:"batchcount"`
}

var globalParams GlobalParams
var loaded = false

func init() {
	load()
}

func load() {
	fmt.Println("global params load")
	file, err := os.Open("params.json")
	if err != nil {
		fmt.Println("param.load | error:", err)
	}
	defer file.Close()
	decoder := json.NewDecoder(file)
	err = decoder.Decode(&globalParams)
	if err != nil {
		fmt.Println("param.load | error:", err)
	}
	loaded = true
}

func GetGlobalParams() GlobalParams {
	if !loaded {
		load()
	}
	return globalParams
}
