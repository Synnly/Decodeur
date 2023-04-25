.PHONY: dechiffreMessageAvecDecalage, clean

main: dechiffreMessage dechiffreMessageAvecDecalage
	@gcc main.c -o main
	@chmod +x main

dechiffreMessage: dechiffreMessageAvecDecalage
	@gcc dechiffreMessage.c -o dechiffreMessage
	@chmod +x dechiffreMessage

dechiffreMessageAvecDecalage:
	@gcc dechiffreMessageAvecDecalage.c -o dechiffreMessageAvecDecalage
	@chmod +x dechiffreMessageAvecDecalage

clean:
	@rm -f main dechiffreMessage dechiffreMessageAvecDecalage

test: main
	@./main TestEtMessagesCodes TEST