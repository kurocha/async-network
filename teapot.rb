
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "1.3"

# Project Metadata

define_project "async-network" do |project|
	project.title = 'Async Network'
	project.summary = 'Abstractions for working with network addresses, sockets and endpoints.'

	project.summary = 'A brief one line summary of the project.'
	
	project.description = <<-EOF
		Async Network description.
	EOF
	
	project.license = "MIT License"
	
	project.add_author 'Samuel Williams', email: 'samuel.williams@oriontransfer.co.nz'
	# project.website = 'http://AsyncNetwork.com/'
	
	project.version = "1.0.0"
end

# Build Targets

define_target 'async-network-library' do |target|
	target.build do
		source_root = target.package.path + 'source'
		
		copy headers: source_root.glob('Async/Network/**/*.hpp')
		
		build static_library: "AsyncNetwork", source_files: source_root.glob('Async/Network/**/*.cpp')
	end
	
	target.depends 'Build/Files'
	target.depends 'Build/Clang'
	
	target.depends :platform
	target.depends "Language/C++11", private: true
	
	target.depends "Library/Async"
	
	target.depends "Build/Files"
	target.depends "Build/Clang"
	
	target.provides "Library/AsyncNetwork" do
		append linkflags [
			->{install_prefix + 'lib/libAsyncNetwork.a'},
		]
	end
end

define_target "async-nework-tests" do |target|
	target.build do |*arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'AsyncNetwork', source_files: test_root.glob('Async/Network/**/*.cpp'), arguments: arguments
	end
	
	target.depends "Language/C++14", private: true
	
	target.depends "Library/UnitTest"
	target.depends "Library/AsyncNetwork"
	

	target.provides "Test/AsyncNetwork"
end

# Configurations

define_configuration 'async-network' do |configuration|
	configuration[:source] = "http://github.com/kurocha/"
	configuration.require "generate-project"
	
	# Provides all the build related infrastructure:
	configuration.require "platforms"
	configuration.require "build-files"
	
	configuration.require "async"
	configuration.require "concurrent"
	configuration.require "time"
	configuration.require "memory"
	
	# Provides unit testing infrastructure and generators:
	configuration.require "unit-test"
	
	# Provides some useful C++ generators:
	configuration.require "generate-cpp-class"
	configuration.require "generate-project"
	configuration.require "generate-travis"
end
