require "rubygems"
require "rspec"
require 'rspec/core/rake_task'
require "yard"

desc "Run all examples"
task RSpec::Core::RakeTask.new('spec')

task :default => "spec"

desc "Generate documentation"
task YARD::Rake::YardocTask.new

task :native => "bin/recommendify"

desc "Compile the native client"
file "bin/recommendify" => FileList['ext/*.c'] do |t|
  %x{gcc -pedantic-errors -Wall -std=c99 -lhiredis -o #{t.name} #{t.prerequisites.join(' ')}}
end
