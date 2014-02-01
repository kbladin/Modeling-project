classdef Connection
    properties
        k; %spring constant
        b; %damper constant
        l; %spring length
        
        %{
        
        p1------Connection------p2
        |
        |
    Connection
        |
        |
        p3------Connection------p4
        

        osv
        
        %}
        p1; %punkt 1 (innehåller massa, position, hastighet)
        p2; %punkt 2
    end
    
    methods
        function applyForce(this,buffer_index)
            this.p1.force = [3,3];
            this.p1
        end
    end
    
end