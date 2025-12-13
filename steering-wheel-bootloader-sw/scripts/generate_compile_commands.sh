pio run -e cm7 -t compiledb && mv compile_commands.json compile_commands_cm7.json
pio run -e cm4 -t compiledb && mv compile_commands.json compile_commands_cm4.json
jq -s 'add' compile_commands_cm7.json compile_commands_cm4.json > compile_commands.json
rm compile_commands_cm4.json compile_commands_cm7.json
