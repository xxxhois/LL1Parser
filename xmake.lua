add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

set_languages("cxx17")

if is_plat("windows") then
    add_cxxflags("/utf-8")
    add_defines("NOMINMAX")
end

target("parser")
    add_files("src/*.cpp")
    add_includedirs("include")
    before_build(
        function (target)
            local project_root = os.projectdir()
            project_root = project_root:gsub("\\", "/")
            target:add("defines", "XMAKE_PROJECT_ROOT=\"" .. project_root .. "\"")
        end
    )
    after_link(
        function (target)
            if is_plat("windows") then
                cprint('${red}Chinese characters may appear garbled in the console. Please set the character set to UTF-8 manually.')
            end
        end
    )