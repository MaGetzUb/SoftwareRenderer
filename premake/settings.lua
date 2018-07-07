local settings = settings or {}

-- Operating system
settings.system = os.host()

-- CPU Architecture
if settings.system == "linux" then
	settings.architecture = io.popen("uname -m", "r"):read("*l")
	if settings.architecture:match("^arm") then
		settings.architecture = "ARM"
	end
elseif os.is64bit() then
	settings.architecture = "x64"
else
	settings.architecture = "x86"
end

return settings
