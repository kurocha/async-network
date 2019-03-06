
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
	target.depends "Language/C++14"
	
	target.depends "Library/Async", public: true
	target.depends "Library/URI", public: true
	
	target.provides "Library/AsyncNetwork" do
		source_root = target.package.path + 'source'
		
		library_path = build static_library: "AsyncNetwork", source_files: source_root.glob('Async/Network/**/*.cpp')
		
		append linkflags library_path
		append header_search_paths source_root
	end
end

define_target "async-nework-tests" do |target|
	target.depends "Language/C++14"
	
	target.depends "Library/Parallel"
	target.depends "Library/UnitTest"
	target.depends "Library/AsyncNetwork"
	
	target.provides "Test/AsyncNetwork" do |*arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'AsyncNetwork', source_files: test_root.glob('Async/Network/**/*.cpp'), arguments: arguments
	end
end

# Configurations

define_configuration "development" do |configuration|
	configuration[:source] = "http://github.com/kurocha/"
	configuration.import "async-network"
	
	# Provides all the build related infrastructure:
	configuration.require "platforms"
	configuration.require "build-files"
	
	# Provides unit testing infrastructure and generators:
	configuration.require "unit-test"
	
	# Provides some useful C++ generators:
	configuration.require "generate-travis"
	configuration.require "generate-project"
	configuration.require "generate-cpp-class"
end

define_configuration "async-network" do |configuration|
	configuration.public!
	
	configuration.require "async"
	configuration.require "uri"
end
