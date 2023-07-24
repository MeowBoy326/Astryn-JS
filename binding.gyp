{
    "targets": [
        {
            "target_name": "memory", 
            "sources": [".\\Memory\\libs\\readwrite.cc",
                        ".\\Memory\\libs\\search.cc",
                        ".\\Memory\\libs\\process.cc",
                        ".\\Memory\\libs\\hreadwrite.cc",
                        ".\\Memory\\libs\\hsearch.cc",
                        ".\\Memory\\libs\\memory.cpp"]
        },
        {
            "target_name": "floatConversion", 
            "sources": [".\\Utils\\libs\\floatConversion.cpp"]
        },
        {
            "target_name": "mouse", 
            "sources": [".\\Input\\libs\\mouse.cpp"]
        },
        {
            "target_name": "keyboard", 
            "sources": [".\\Input\\libs\\keyboard.cpp"]
        },
        {   "target_name": "winapi", 
            "sources": [".\\WinAPI\\libs\\jsMemoryapi.cc",
                        ".\\WinAPI\\libs\\jsProcessthreadsapi.cc",
                        ".\\WinAPI\\libs\\winapi.cpp"]
        },
    ]
}
