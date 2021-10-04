package main

import(
	"log"
	"fmt"
	"github.com/tarm/serial"
	"bufio"
	"flag"
	"os"
	"github.com/fatih/color"
)

var verb bool

func init(){
	flag.BoolVar(&verb, "v", false, "display detailed output")
}

func main(){
	flag.Parse()
	if len(os.Args) == 2 || len(os.Args) == 3{
		com := os.Args[len(os.Args) - 1]
		fmt.Println("Running tests...")

		c := &serial.Config{Name: com, Baud: 115200}
		s, err := serial.OpenPort(c)
		if err != nil{
			log.Fatal(err)
		}

		str := make([]string, 10, 98)

		scanner := bufio.NewScanner(s)

		set_shell_params(s, scanner)

		test := 0
		passed := make([]bool, 9)
		if verb{
			fmt.Println("Deleting previously generated keys...")
		}
		reset(s, scanner, verb, passed, test)
		test = 8
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Generating 10 keys...")
		}
		keygen(s, scanner, str, verb, passed)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Retrieving generated keys...")
		}
		getkeys(s, scanner, verb, passed)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Checking keys are different...")
		}
		check(s, scanner, str, verb, passed)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Attempting to generate extra key...")
		}
		keygenext(s, scanner, verb, passed)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Attempting to sign message with wrong length...")
		}
		sign := ""
		msg := "565656565656565656565656565656565656565656565656565656565656565"
		signature(s, scanner, msg, &sign, str, verb, passed, 5)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Attempting to sign message with correct length...")
		}
		msg = "5656565656565656565656565656565656565656565656565656565656565656"
		signature(s, scanner, msg, &sign, str, verb, passed, 6)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Attempting to verify generated signature...")
		}
		verify(s, scanner, msg, sign, str, verb, passed)
		if verb{
			fmt.Printf("\n\n")
		}

		if verb{
			fmt.Println("Deleting keys...")
		}
		reset(s, scanner, verb, passed, test)
		if verb{
			fmt.Printf("\n\n")
		}

		reset_shell_params(s)

		//pass := "\u2714"
		//fail := "\u274c"

		color.HiCyan("RESULTS:")
		fmt.Println("----------------------------------------")
		if verb{
			fmt.Printf("Delete previous keys..........")
			if passed[0]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Generate 10 keys..............")
			if passed[1]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Retrieve generated keys.......")
			if passed[2]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Check keys are different......")
			if passed[3]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Try to generate extra key.....")
			if passed[4]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Sign msg with wrong length....")
			if passed[5]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Sign correct msg..............")
			if passed[6]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Verify signature..............")
			if passed[7]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}

			fmt.Printf("Delete keys...................")
			if passed[8]{
				color.HiGreen("PASSED")
			}else{
				color.Red("FAILED")
			}
		}
		cont := 0
		for i:= 0; i < 9; i++{
			if passed[i]{
				cont++
			}
		}
		color.HiMagenta("Total.........................%d/9", cont)
		fmt.Println("----------------------------------------")
	}else{
		fmt.Println("Usage: .\\test.exe [-v] COMport")
	}
}