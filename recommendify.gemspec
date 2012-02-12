# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)

Gem::Specification.new do |s|
  s.name        = "recommendify"
  s.version     = "0.1.0"
  s.date        = Date.today.to_s
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Paul Asmuth"]
  s.email       = ["paul@paulasmuth.com"]
  s.homepage    = "http://github.com/paulasmuth/recommendify"
  s.summary     = %q{Distributed item-based "Collaborative Filtering" with ruby and redis}
  s.description = %q{Distributed item-based "Collaborative Filtering" with ruby and redis}
  s.licenses    = ["MIT"]
  
  s.add_dependency "redis", ">= 2.2.2"
  
  s.add_development_dependency "rspec", "~> 2.8.0"

  s.files         = `git ls-files`.split("\n") - [".gitignore", ".rspec", ".travis.yml"]
  s.test_files    = `git ls-files -- spec/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib"]
end
