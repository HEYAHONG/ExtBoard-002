--LuaTools需要工程名及版本号
PROJECT = "base"
VERSION = "1.0.0"

--加载sys与sysplus库,这两个库均由官方提供
_G.sys = require("sys")
_G.sysplus = require("sysplus")

--APP信息表
local app={}

----将mobile信息存储在app中
if mobile then
	--自动识别，优先使用SIM0
	mobile.simid(2,true)
	app.mobile={}
	app.mobile.sim={}
	sys.subscribe("SIM_IND",function(status,value)
		--将SIM消息存储在app中
		log.info("SIM",status)
		if status=="SIM_WC" then
			log.info("SIM","write counter",value)
		end
		if status=="RDY" then
			app.mobile.sim.rdy=true
		end
		if status=="NORDY" then
			app.mobile.sim.rdy=false
		end
		if status=="SIM_PIN" then
			app.mobile.sim.rdy=false
			app.mobile.sim.pin=true
		end
		if status=="GET_NUMBER" then
			log.info("SIM","number",mobile.number(0))
		end
	end)
	app.mobile.ip={}
	sys.subscribe("IP_READY",function(ip,adapter)
		--将IP信息存储在app中
		log.info("mobile","IP_READY",ip)
		app.mobile.ip.ip=ip
		app.mobile.ip.adapter=adapter
	end)
	sys.subscribe("IP_LOSE",function(adapter)
		--将IP信息存储在app中
		log.info("mobile","IP_LOSE")
		app.mobile.ip.ip=nil
		app.mobile.ip.adapter=adapter
	end)
	app.mobile.ntp={}
	sys.subscribe("NTP_UPDATE",function()
		log.info("mobile","time",os.date())
		app.mobile.ntp.update=true
	end)
end

----将APP信息加入_G(可全局访问)
_G.app=app


--启动初始化任务,在任务中完成剩余初始化任务
sys.taskInit(function()
	log.info("Main init started!");
	--遍历文件
	ret, files = io.lsdir("/luadb/",50);--默认列出50个文件,一般来说够用，TODO:如果文件过多需要进行多次遍历
	--加载lib文件
	if ret and files then
		for index,file in ipairs(files)
		do
			local filename=file["name"];
			if filename then
				if string.find(filename,".lib",-4) then
					--lib文件
					log.info("Load lib ",filename);
					local m=dofile("/luadb/" .. filename)
					if m then
						local m_name=string.sub(filename,0,-5);
						log.info("module ".. m_name .. " is loaded！")
						--将lib放入_G中,注意:lib文件最好不要与系统库或者官方扩展重名
						_G[m_name]=m
					end
				end
			end
		end
	end
	--加载init文件
	if ret and files then
		for index,file in ipairs(files)
		do
			local filename=file["name"];
			if filename then
				if string.find(filename,".init",-5) then
					--init文件
					log.info("Load init ",filename);
					local m=dofile("/luadb/" .. filename)
					if m and m.init and type(m.init) == "function" then
						--加载init函数
						m.init()
					end
				end
			end
		end
	end
end)

--启动系统运行，调用此函数后，CPU将处于对协程的调度中，此语句后的语句将不执行
sys.run()
