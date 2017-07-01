
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "1.3"

define_project "Async::Network" do |project|
	project.add_author "Samuel Williams"
	project.license = "MIT License"

	project.version = "1.0.0"
end

define_target "async-network" do |target|
	target.build do |environment|
		source_root = target.package.path + 'source'
		
		copy headers: source_root.glob('Async/Network/**/*.hpp')
		
		build static_library: "AsyncNetwork", source_files: source_root.glob('Async/Network/**/*.cpp')
	end
	
	target.depends :platform
	target.depends "Language/C++11", private: true
	
	target.depends "Library/Async"
	
	target.depends "Build/Files"
	target.depends "Build/Clang"
	
	target.provides "Library/AsyncNetwork" do
		append linkflags ->{install_prefix + "lib/libAsyncNetwork.a"}
	end
end

define_target "async-nework-tests" do |target|
	target.build do |environment|
		test_root = target.package.path + 'test'
		
		run tests: "AsyncNetwork", source_files: test_root.glob('Async/Network/**/*.cpp')
	end
	
	target.depends "Language/C++11", private: true
	
	target.depends "Library/UnitTest"
	target.depends "Library/AsyncNetwork"
	
	target.provides "Test/AsyncNetwork"
end

define_configuration "test" do |configuration|
	configuration[:source] = "http://github.com/kurocha/"
	
	configuration.require "platforms"
	configuration.require "build-files"
	
	configuration.require "async"
	configuration.require "concurrent"
	configuration.require "time"
	configuration.require "memory"
	
	configuration.require "unit-test"
	
	configuration.require "language-cpp-class"
end
