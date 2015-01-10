

def GeneratePolicyCompilationTest
  def Recurse(buildup, level, usedPolicies, count)
    defaultPolicies = ['detail::DefaultPoliciesArg']
    syncPolicies = ['NoSync', 'InstanceLock', 'ProcesswideLock']
    storagePolicies = ['ArrayStorage', 'SortedArrayStorage', 'SetStorage', 'FixedArrayStorage<20>']
    resultPolicies = ['NoResult', 'All<true>', 'Any<true>']

    def RecurseOnPolicy(buildup, level, policies, name, usedPolicies, count)
      usedPolicies[name] = true
      policies.each {|p|
        nextBuildup = (buildup == '') ? p : "#{buildup}, #{p}"
        Recurse(nextBuildup, level+1, usedPolicies, count)
      }
      usedPolicies[name] = nil
    end

    if level == 3 then
      puts "\tSignal1<int, Policies<#{buildup}>> Sig#{count[0]};"
      count[0] += 1
    else
      #puts "Recursing to lvl #{level}"
      Recurse(buildup, level+1, usedPolicies, count)
      
      RecurseOnPolicy(buildup, level, syncPolicies, 'Sync', usedPolicies, count) if usedPolicies['Sync'] == nil
      RecurseOnPolicy(buildup, level, storagePolicies, 'Storage', usedPolicies, count) if usedPolicies['Storage'] == nil
      RecurseOnPolicy(buildup, level, resultPolicies, 'Result', usedPolicies, count) if usedPolicies['Result'] == nil
      RecurseOnPolicy(buildup, level, defaultPolicies, 'Default', usedPolicies, count) if usedPolicies['Default'] == nil
    end
  end

  Recurse('', 0, {}, [0])
  
end

GeneratePolicyCompilationTest()