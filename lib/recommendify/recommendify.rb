module Recommendify

  DEFAULT_MAX_NEIGHBORS = 50

  @@redis = nil

  def self.redis=(redis)
    @@redis = redis
  end

  def self.redis
    return @@redis unless @@redis.nil?
    raise "redis not configured! - Recommendify.redis = Redis.new"
  end

  def self.capitalize(str_or_sym)
  	str = str_or_sym.to_s.each_char.to_a
  	str.first.upcase + str[1..-1].join("").downcase
  end

  def self.constantize(klass)
    Object.module_eval("Recommendify::#{klass}", __FILE__, __LINE__)
  end

end