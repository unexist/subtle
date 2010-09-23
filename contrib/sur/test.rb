#
# @package sur
#
# @file Sublet test functions
# @author Christoph Kappel <unexist@dorfelite.net>
# @version $Id$
#
# This program can be distributed under the terms of the GNU GPL.
# See the file COPYING.
#

require "subtle/subtlext"

module Subtle # {{{
  module Sur # {{{
    module Test # {{{
      # Sublet class
      class Sublet # {{{
        # Update interval
        attr_accessor :interval

        # Sublet data
        attr_accessor :data

        # Watch path
        attr_accessor :path

        # Visibility
        attr_accessor :visible

        # Event list
        SUBLET_EVENTS = {
          :run        => 1,
          :mouse_down => 4,
          :mouse_over => 1,
          :mouse_out  => 1
        }

        # Hook list
        SUBLET_HOOKS = [
          :client_create,
          :client_configure,
          :client_focus,
          :client_kill,
          :tag_create,
          :tag_kill,
          :view_create,
          :view_configure,
          :view_focus,
          :view_kill,
          :exit
        ]

        ## Subtle::Sur::Test::Sublet::initialize {{{
        # Initialize a Sublet
        #
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet.new
        #   => #<Subtlext::Sublet:xxx>

        def initialize
          @visible  = true
          @interval = 60
        end # }}}

        ## Subtle::Sur::Test::Sublet::method_missing {{{
        # Dispatcher for Sublet instance
        #
        # @param [Symbol, #read]  meth  Method name
        # @param [Array,  #read]  args  Argument array
        #
        # @since 0.1

        def method_missing(meth, *args)
          ret = nil

          # Check if symbol is a method or a var
          if(self.respond_to?(meth))
            ret = self.send(meth, args)
          else
            sym = ("@" + meth.to_s).to_sym #< Construct symbol

            # Check getter or setter
            if(meth.to_s.index("="))
              sym = sym.to_s.chop.to_sym

              self.instance_variable_set(sym, args.first)
            elsif(self.instance_variable_defined?(sym))
              ret = self.instance_variable_get(sym)
            end
          end

          ret
        end # }}}

        ## Subtle::Sur::Test::Sublet::interval {{{
        # Get the interval of a Sublet
        #
        # @return [Fixnum] Interval time
        #
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet.new.interval
        #   => 60

        def interval
          @interval
        end # }}}

        ## Subtle::Sur::Test::Sublet::interval= {{{
        # Set the interval of a Sublet
        #
        # @param [Fixnum, #read]  interval  Interval time
        #
        # @raise [String] Sublet error
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet.new.interval = 60
        #   => nil

        def interval=(interval)
          raise ArgumentError.new("Unknown value type") unless(interval.is_a?(Fixnum))
          @interval = interval
        end # }}}

        ## Subtle::Sur::Test::Sublet::data {{{
        # Get the data of a Sublet
        #
        # @return [String] Sublet data
        #
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet.new.data
        #   => "Subtle"

        def data
          @data
        end # }}}

        ## Subtle::Sur::Test::Sublet::data= {{{
        # Set the data of a Sublet
        #
        # @param [String, #read}  data  Sublet data
        #
        # @raise [String] Sublet error
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet.new.data = "subtle"
        #   => nil

        def data=(data)
          raise ArgumentError.new("Unknown value type") unless(data.is_a?(String))
          @data = data
        end # }}}

        ## Subtle::Sur::Test::Sublet::background= {{{
        # Set the background of a Sublet
        #
        # @param [String, #read}  value  Sublet background
        #
        # @raise [String] Sublet error
        # @since 0.2
        #
        # @example
        #   Subtle::Sublet.new.background = "#ffffff"
        #   => nil

        def background=(value)
          unless(value.is_a?(String) or value.is_a?(Object))
            raise ArgumentError.new("Unknown value type") 
          end
        end # }}}

        ## Subtle::Sur::Test::Sublet::geometry {{{
        # Get geometry of a Sublet
        #
        # @return [String] Sublet geometry
        #
        # @raise [String] Sublet error
        # @since 0.2
        #
        # @example
        #   Subtle::Sublet.new.geometry
        #   => #<Subtlext::Geometry:xxx>

        def geometry
          Subtlext::Geometry.new(10, 10, 20, 20)
        end # }}}

        ## Subtle::Sur::Test::Sublet::watch {{{
        # Watch a file found on path
        #
        # @raise [String] Sublet error
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet.new.watch("/tmp/watch")
        #   => nil

        def watch(path)
          raise ArgumentError.new("Unknown value type") unless(path.is_a?(String))
          raise "File not found" unless(File.exist?(path))
          @path = path
        end # }}}

        ## Subtle::Sur::Test::Sublet::unwatch {{{
        # Stop watching a file
        #
        # @since 0.0
        #
        # @example
        #   Subtle::Sublet::unwatch
        #   => nil

        def unwatch
          @path = nil
        end # }}}

        ## Subtle::Sur::Test::Sublet::show {{{
        # Show sublet on panel
        #
        # @since 0.2
        #
        # @example
        #   Subtle::Sublet.new.show
        #   => nil

        def show
          @visible = true
        end # }}}

        ## Subtle::Sur::Test::Sublet::hide {{{
        # Hide sublet from panel
        #
        # @since 0.2
        #
        # @example
        #   Subtle::Sublet.new.show
        #   => nil

        def hide
          @visible = false
        end # }}}

        ## Subtle::Sur::Test::Sublet::hidden? {{{
        # Whether sublet is hidden
        #
        # @since 0.2
        #
        # @example
        #   Subtle::Sublet.new.show
        #   => nil

        def hidden?
          !@visible
        end # }}}

        ## Subtle::Sur::Test::Sublet::configure {{{
        # Configure block for Sublet
        #
        # @param [Symbol, #read]  name  Sublet name
        #
        # @yield [Object] New Sublet
        #
        # @raise [String] Sublet error
        # @since 0.1
        #
        # @example
        #   configure :sublet do |s|
        #     s.interval = 60
        #   end

        def configure(name, &blk)
          raise LocalJumpError.new("No block given") unless(block_given?)
          raise ArgumentError.new("Unknown value type") unless(name.is_a?(Symbol))

          # Add configure method
          $sublet.class.send(:define_method, :configure, blk)
        end # }}}

        ## Subtle::Sur::Test::Sublet::on {{{
        # Event block for Sublet
        #
        # @param [Symbol, #read]  event  Event name
        #
        # @yield [Proc] Event handler
        #
        # @raise [String] Sublet error
        # @since 0.1
        #
        # @example
        #   on :event do |s|
        #     puts s.name
        #   end

        def on(event, &blk)
          raise LocalJumpError.new("No block given") unless(block_given?)
          raise ArgumentError.new("Unknown value type") unless(event.is_a?(Symbol))

          # Get proc information
          arity = blk.arity
          sing  = self.class

          # Check events
          if(SUBLET_EVENTS.has_key?(event))
            need = SUBLET_EVENTS[event]

            if(-1 == arity || (1 <= arity && need >= arity))
              sing.send(:define_method, event, blk)
            else
              raise "Wrong number of arguments (%d for %d)" % [ arity, need ]
            end
          end

          # Check hooks
          if(SUBLET_HOOKS.include?(event))
            sing.send(:define_method, event, blk)
          end
        end # }}}

        ## Subtle::Sur::Test::Sublet::helper {{{
        # Helper block for Sublet
        #
        # @yield [Proc] Helper handler
        #
        # @raise [String] Sublet error
        # @since 0.1
        #
        # @example
        #   helper do |s|
        #     def test
        #       puts "test"
        #     end
        #   end

        def helper(&blk)
          raise LocalJumpError.new("No block given") unless(block_given?)

          # Yield and eval block
          yield(self)

          self.instance_eval(&blk)
        end # }}}
      end # }}}

    # Test class for Dummy
    class Dummy # {{{
      ## Subtle::Sur::Test::Dummy::method_missing {{{
      # Dispatcher for Dummy instance
      #
      # @param [Symbol, #read]  meth  Method name
      # @param [Array,  #read]  args  Argument array
      #
      # @since 0.1

      def method_missing(meth, *args)
        ret = nil

        # Check if symbol is a method or a var
        if(self.respond_to?(meth))
          ret = self.send(meth, args)
        else
          sym = ("@" + meth.to_s).to_sym #< Construct symbol

          # Check getter or setter
          if(meth.to_s.index("="))
            sym = sym.to_s.chop.to_sym

            self.instance_variable_set(sym, args.first)
          else
            ret = self.instance_variable_get(sym)
            ret = self if(ret.nil?)
          end
        end

        ret
      end # }}}
    end # }}}

      ## Subtle::Sur::Test::run {{{
      # Run test for every file in args
      #
      # @param  [Array, #read]  args  Args array
      #
      # @example
      #   Sur::Test::Subtle.new.run(args)
      #   => nil

      def self.run(args)
        args.each do |arg|
          # Load sublet
          if(File.exist?(arg))
            begin
              sublet = Subtle::Sur::Test::Sublet.new

              # Eval sublet in anonymous module
              sublet.instance_eval(File.read(arg))
            rescue => err
              puts ">>> WARNING: Couldn't load sublet: %s" % [
                err.message
              ]

              unless(error.is_a?(RuntimeError))
                puts error.backtrace
              end
            end

            # Check if sublet exists
            unless(sublet.nil?)
              methods = []
              dummy   = Subtle::Sur::Test::Dummy.new

              # Configure and run sublet
              sublet.configure(sublet)
              sublet.run(sublet) if(sublet.respond_to?(:run))

              # Sanitize
              if(!sublet.instance_variable_defined?("@interval") or 0 >= sublet.interval)
                sublet.interval = 60
              end

              # Collect events
              SUBLET_EVENTS.each do |k, v|
                if(sublet.respond_to?(k))
                  methods.push({
                    :name      => k,
                    :arity     => sublet.method(k).arity,
                    :singleton => false
                  })
                end
              end

              # Collect hooks
              SUBLET_HOOKS.each do |k|
                if(sublet.respond_to?(k))
                  methods.push({
                    :name      => k,
                    :arity     => sublet.method(k).arity,
                    :singleton => false
                  })
                end
              end

              # Collect singleton methods
              sublet.singleton_methods.each do |k|
                methods.push({
                  :name      => k,
                  :arity     => sublet.method(k).arity,
                  :singleton => true
                })
              end

              begin
                puts "------------------"
                puts " %s" % [ sublet.name ]
                puts "------------------"

                # Show instance variables
                sublet.instance_variables.each do |v|
                  puts " %s = %s" % [ v, sublet.instance_variable_get(v) ]
                end

                puts "------------------"

                # Show method list
                i = 1
                methods.each do |m|
                  puts " (%d) %s %s" % [
                    i,
                    m[:name],
                    (m[:singleton] ? "(helper)" : "")
                  ]

                  i += 1
                end

                puts " (0) Quit"
                puts "------------------"
                puts ">>> Select one method:\n"
                printf ">>> "

                num = STDIN.readline.to_i

                begin
                  if(0 < num && methods.size >= num)
                    meth = methods[num - 1]

                    # Check proc arity
                    case meth[:arity]
                      when 2 then
                        sublet.send(meth[:name], sublet, dummy)
                      when 4 then
                        sublet.send(meth[:name], sublet, 5, 5, 1)
                      when 0 then
                        sublet.send(meth[:name])
                      else
                        sublet.send(meth[:name], sublet)
                    end
                  end
                rescue => error
                  puts ">>> ERROR: #{error}"

                  unless(error.is_a?(RuntimeError))
                    puts error.message
                    puts error.backtrace
                  end
                end
              end while(0 != num)
            end
          end
        end
      end # }}}
    end # }}}
  end # }}}
end # }}}

# vim:ts=2:bs=2:sw=2:et:fdm=marker
