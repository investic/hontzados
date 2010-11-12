<?php
// $Id: hontza.tpl.php

?>
<?php global $user;?>
<?php if ($user->name):?>
  <div class="menu-user"><li class="user-m"><?php print $user->name .'</li>'. $enlaces ?></div>
  <?php else: ?>
    <li><?php print $enlaces ?></li>  
<?php endif; ?>
