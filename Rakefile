require "rubygems"
require "rspec"
require 'rspec/core/rake_task'
require "yard"

desc "Run all examples"
task RSpec::Core::RakeTask.new('spec')

task :default => "spec"

desc "Generate documentation"
task YARD::Rake::YardocTask.new


desc "Compile the native client"
task :build_native do
  exec "cd ext && ruby extconf.rb && make"
end