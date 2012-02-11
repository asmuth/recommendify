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
  out_dir = ::File.expand_path("../bin", __FILE__)  
  src_dir = ::File.expand_path("../src", __FILE__)  
  %x{mkdir -p #{out_dir}}
  %x{g++ -ansi -pedantic-errors -Wall #{src_dir}/recommendify.cpp -lhiredis -o #{out_dir}/recommendify}
end